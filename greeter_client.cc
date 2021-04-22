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

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using grpc::ClientReader;
using grpc::ClientWriter;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::ProductID;
using helloworld::ProductValue;
using helloworld::Product;
using helloworld::Result;
using helloworld::Greeter;

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

	std::string SayHelloAgain( const std::string &user)
	{
		HelloRequest request; 
		request.set_name( user);
		HelloReply reply;
		ClientContext context; 
		Status status = stub_->SayHelloAgain(&context, request, &reply);
		if( status.ok())
		{
			return reply.message();
		}
		else 
		{
			std::cout<<status.error_code()<<" : "<<status.error_message()<<std::endl;
			return "RPC Failed";
		
		
		}
	}

	void GetValue ( const std::string &key) 
	{
		ProductID p; 
		p.set_key( key);
		ClientContext context; 
		std::cout<<"The key value is set"<<std::endl;


		ProductValue val;

		 std::unique_ptr<ClientReader<ProductValue> > reader(stub_->GetValue(&context, p));

		 while( reader->Read(&val))
		 {
		 	std::cout<<"The value read is "<<val.value()<<std::endl;
		 }

		 Status status = reader->Finish();

		 if( status.ok())
		 {
		 		std::cout<<"GetValue Succeeded"<<std::endl;
		 }
		 else 
		 std::cout<<"GetValue failed"<<std::endl;
	}

	int SetValue( std::string &key, std::vector<std::string> &values )
	{
		Product p; 

		ClientContext context; 
		Result r;

		std::unique_ptr<ClientWriter<Product> > writer(stub_->SetValue(&context, &r));

		p.set_data( key);
		if (!writer->Write(p)) 
		{
			std::cout<<"Error in setvalue"<<std::endl;
			return -1;
		}
		for( auto &t:values)
		{
			
			p.set_data( t);
			if (!writer->Write(p)) 
			{
				std::cout<<"Error in setvalue"<<std::endl;
				return -1;
			}
		}

		writer->WritesDone();
		Status status = writer->Finish();
		if (status.ok()) {
		std::cout<<"Set value finished successfully"<<std::endl;

		return 0;
		}

		else 
		{
			
			std::cout<<"Error in set value "<<std::endl;
			return -1;
		}

	}










 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  GreeterClient greeter(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;
	
	reply = greeter.SayHelloAgain(user);
	std::cout<< "Greeter Received "<<reply<<std::endl;

	std::cout<<"Get values called"<<std::endl;

	std::string x("Shoes");
	greeter.GetValue(x);

	std::vector<std::string> prices { "11","12","55","66"};
	std::string product_id( "Reebok"); 

	int flag = greeter.SetValue( product_id, prices); 
	if( flag == -1) 
	{
		std::cout<<"Failed setvalue"<<std::endl;
	}

	greeter.GetValue( product_id); 



  return 0;
}
