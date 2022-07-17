
out:gen
	cd "/home/zxx/桌面/Code/WebMail/ProtoBuffer/"
	g++ *.cc test.cpp -w -o test -lprotobuf 
	./test

gen:
	protoc --cpp_out=./ *.proto

clean:
	rm -f *.cc *.h