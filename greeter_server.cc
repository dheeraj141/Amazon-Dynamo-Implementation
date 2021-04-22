/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::Status;
using grpc::ServerWriter;
using helloworld::HelloRequest;
using helloworld::HelloReply;

using helloworld::ProductID;
using helloworld::ProductValue;
using helloworld::Product;
using helloworld::Result;
using helloworld::Greeter;



std::unordered_map<std::string , std::vector<std::string>> product_info( { 
																																						{ "Watch", { "1","2","3","4"}},
																																						{ "Shoes", { "5","6","7","8"}},
																																						{ "Dress", { "9","10","11","12"}}
																																						});


// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }

	Status SayHelloAgain( ServerContext* context, const HelloRequest* request, HelloReply *reply) override {

	std::string prefix("Hello again ");
	reply->set_message(prefix + request->name());
	return Status::OK;
	}

  
	Status SetValue( ServerContext* context, ServerReader<Product>* reader, Result *reply) override {

	std::vector<std::string> product_value; 
	std::string product_key;

	Product p;

	bool flag = 0; 

	while( reader->Read(&p))
	{
		if( flag == 0)
		{
			product_key = p.data(); 
			flag =1;
		}
		else
		{
			product_value.push_back( p.data()); 
		}
	}

	product_info.insert( { product_key, product_value});

	reply->set_passfail("true");

	return Status::OK;


		
	}


	Status GetValue( ServerContext* context, const ProductID *id, ServerWriter<ProductValue> *writer) override {

	auto it = product_info.find(id->key());

	ProductValue p;

	if( it != product_info.end())
	{
		for( auto &t: it->second)
		{
			p.set_value( t);
			writer->Write( p);
		}
	}

	return Status::OK;
	}

		

	







};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
