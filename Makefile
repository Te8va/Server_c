.PHONY: all
all: db files watchfiles
db:
	g++ db.cpp HTTP.cpp working_with_db.cpp  -o db.cgi
files:
	g++ db.cpp HTTP.cpp working_with_files.cpp -o uploadFile.cgi
watchfiles:
	g++ HTTP.cpp watch_files.cpp -o watchFiles.cgi
clean:
	rm *.cgi