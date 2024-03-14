docker run -w /home/project -dit --name %1 --mount type=bind,source=.,target=/home/project ubuntu:latest
docker start %1
docker exec -it %1 sh -c "./DownloadDependencies.bat"
docker attach %1