docker run -w /home/project -dit --name ubuntu --mount type=bind,source=.,target=/home/project ubuntu:latest
docker start ubuntu
docker exec -it ubuntu sh -c "./DownloadDependencies.bat"
docker attach ubuntu