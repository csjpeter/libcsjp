/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2017 Csaszar, Peter
 */

#include <unistd.h>
#include <stdio.h>

#include <csjp_owner_container.h>
//#include <csjp_string.h>
#include <csjp_file.h>

#include <csjp_signal.h>
#include <csjp_server.h>
#include <csjp_client.h>
#include <csjp_epoll_control.h>

//#include <csjp_epoll.h>
//#include <csjp_owner_container.h>

//#define LOG_DIR "/var/log/csjp-nc"
#define LOG_DIR "/var/log"


class NCClient : public csjp::Client
{
public:
	NCClient(const char *name, unsigned port) :
		csjp::Client(name, port) {}
	virtual ~NCClient() {}

	void close(){ Client::close(); }

	void sendStdIn()
	{
		char buff[4096];
		size_t readCount = 0;
		while(true){
			readCount = fread(buff, 1, sizeof(buff), stdin);
			if(!readCount){
				if(feof(stdin))
					break;
				if(ferror(stdin))
					throw csjp::FileError(errno, "Failed to read stdin.");
			}
			send(csjp::Str(buff, readCount));
		}
	}
};

class NCServer : public csjp::Server
{
public:
	NCServer(csjp::Listener & l) : csjp::Server(l) {}
	virtual ~NCServer() {}

	void close(){ Server::close(); }

	virtual void dataReceived() {
		csjp::String data = receive(bytesAvailable);
		size_t writeCount = 0;
		while(true){
			writeCount = fwrite(data.c_str()+writeCount, 1,
					data.length-writeCount, stdout);
			if(!writeCount){
				if(ferror(stdin))
					throw csjp::FileError(errno, "Failed to write stdout.");
				if(feof(stdin))
					throw csjp::FileError(errno,
							"Unexpected EOF while writing to stdout.");
				break;
			}
		}
	}
};

bool operator<(const NCServer & lhs, const NCServer & rhs)
{
        return &lhs < &rhs;
}       

bool operator<(const csjp::Socket & lhs, const NCServer & rhs)
{       
        return &lhs < &rhs;
}       

bool operator<(const NCServer & lhs, const csjp::Socket & rhs)
{
        return &lhs < &rhs;
}

class NCListener : public csjp::Listener
{
public:
        NCListener(const char * ip, unsigned port,
                        csjp::OwnerContainer<NCServer> & businessServers,
                        csjp::EPollControl & epoll,
                        unsigned incomingConnectionQueueLength = 0) :
                csjp::Listener(ip, port, incomingConnectionQueueLength),
                businessServers(businessServers),
                epoll(epoll)
        {}
        virtual ~NCListener() {}

        virtual void dataReceived()
        {
                csjp::Object<NCServer> server(new NCServer(*this));
                epoll.add(*server);
                businessServers.add(server.release());
                epoll.remove(*this); // No further connection to accept
        }
        virtual void readyToSend() {} // should be removed since has no sense here

private:
        csjp::OwnerContainer<NCServer> & businessServers;
        csjp::EPollControl & epoll;
};


int main(int argc, char* args[])
{
        (void)argc;

        /* Initialize logging facility */
        csjp::setBinaryName(args[0]);
        //csjp::setLogDir(LOG_DIR);
        //csjp::verboseMode = true;

        /* Checking command line arguments */
        const char * ip = "";
        csjp::UInt port = 0;
        bool listener = false;
        int argi = 1;
        while(argi < argc){
                if(2 <= argc - argi && (
                                  !strcmp(args[argi], "--ip") ||
                                  !strcmp(args[argi], "-i"))){
                        ip = args[argi+1];
                        argi += 2;
                        continue;
                }
                if(2 <= argc - argi && (
                                  !strcmp(args[argi], "--port") ||
                                  !strcmp(args[argi], "-p"))){
                        port = args[argi+1];
                        argi+=2;
                        continue;
                }
                if(1 <= argc - argi && (
                                  !strcmp(args[argi], "--listen") ||
                                  !strcmp(args[argi], "-l"))){
                        listener = true;
                        argi++;
                        continue;
                }
                if(1 <= argc - argi && (
                                  !strcmp(args[argi], "--help") ||
                                  !strcmp(args[argi], "-h"))){
                        printf(
                "Demo primitive netcat implementaion for network debugging.\n"
                "All Right Reserved (C) 2017 Császár Péter, Hungary\n\n"
                "Usage:\n"
                "       %s [-l|--listen] <-i|--ip ipv4_addr> <-p|--port portnum>\n"
		"	%s <-h|--help>\n\n"
                , args[0], args[0]);
                        return 0;
                }
                fprintf(stderr, "Bad arguments given.\n");
                return 1;
        }
	if(ip[0] == 0 || port == 0){
		fprintf(stderr, "Ip and port must be specified.\n");
		return 1;
	}

        csjp::File logDir(LOG_DIR);
        if(!logDir.exists())
                logDir.createDir();

	try {
		if(!listener) {
			try {
				NCClient client(ip, port);
				client.sendStdIn();
			} catch (csjp::Exception & e) {
				FATAL(e.what());
				//EXCEPTION(e);
				return 1;
			}
			return 0;
		}

		/* Setting up network listening */
		LOG("Setting up listener socket");

		csjp::OwnerContainer<NCServer> businessServers;
		csjp::EPollControl epoll(5);
		csjp::Signal pipeSignal(SIGPIPE, csjp::Signal::sigpipeHandler);
		NCListener listener(ip, port, businessServers, epoll);
		epoll.add(listener);

		/* Main loop */
		LOG("Entering main loop (listening for incoming connections and requests)");
		bool closedBypeer = false;
		while(!csjp::Signal::sigTermReceived && !csjp::Signal::sigKillReceived){
			// max 0.01 sec wait, event handling
			for(auto event : epoll.waitAndControl(1)){
				DBG("ControlEvent received: %", event);
				switch(event.code){
					case csjp::EPollControl::ControlEventCode::Exception:
						FATAL(event.exception.what());
						//EXCEPTION(event.exception);
					case csjp::EPollControl::ControlEventCode::ClosedByPeer:
					case csjp::EPollControl::ControlEventCode::ClosedByHost:
						businessServers.remove(event.socket);
						closedBypeer = true;
						break;
					default :
						DBG("Unhandled event received: %", event);
						;
				}
				if(closedBypeer)
					break;;
			}
			if(closedBypeer)
				break;;
		}
		LOG("Normal exit");
	} catch (csjp::Exception & e) {
		FATAL(e.what());
		//EXCEPTION(e);
		return 1;
	}

	return 0;
}

