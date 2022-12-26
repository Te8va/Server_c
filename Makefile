.PHONY: all
all: db files watchfiles session
db:
	g++ db.cpp HTTP.cpp working_with_db.cpp  -o db.cgi
files:
	g++ db.cpp HTTP.cpp working_with_files.cpp -o uploadFile.cgi
watchfiles:
	g++ HTTP.cpp watch_files.cpp -o watchFiles.cgi
session:
	g++ HTTP_session.cpp working_with_session.cpp HTTP.cpp -l tacopie -l cpp_redis -o session.cgi
clean:
	rm *.cgi