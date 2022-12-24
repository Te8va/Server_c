.PHONY: all
all: db files
db:
	g++ db.cpp HTTP.cpp working_with_db.cpp  -o db.cgi
files:
	g++ db.cpp HTTP.cpp working_with_files.cpp -o uploadFile.cgi
clean:
	rm *.cgi