#ifndef TYPE_H
#define TYPE_H

#include <memory>
#include <list>
#include <thread>

struct Request;
using Request_ptr = std::shared_ptr<Request>;

struct RequestReplyServer;
using RequestReplyServer_ptr = std::shared_ptr<RequestReplyServer>;

struct RequestReplyClient;
using RequestReplyClient_ptr = std::shared_ptr<RequestReplyClient>;

struct  RequestReplyServer2;
using   RequestReplyServer2_ptr = std::shared_ptr<RequestReplyServer2>;

class Cryption;
using  Cryption_ptr = std::shared_ptr<Cryption>;

class Client;
using Client_ptr= std::shared_ptr<Client>;

using Thread_ptr= std::shared_ptr<std::thread>;

using ClientList = std::list<Client_ptr>;

class Stream;
using Stream_ptr= std::shared_ptr<Stream>;

#endif