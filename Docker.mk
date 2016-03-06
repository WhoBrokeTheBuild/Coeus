.PHONY: docker docker-base docker-HTTP docker-run-HTTP docker-FTP
	
docker: docker-base docker-HTTP docker-FTP

docker-base:
	docker build --tag="coeus" .

docker-HTTP:
	docker build --tag="coeus-http" HTTP/

docker-run-HTTP:
	docker run -d -t -p 8080:8080 coeus-http

docker-FTP:
	docker build --tag="coeus-ftp" FTP/
