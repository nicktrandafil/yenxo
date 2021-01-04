#!/bin/bash

IMG=$1
shift

if [ -z "$IMG" ]; then
    IMG=serialize
fi

CONTAINER=$(docker run --rm -d -P -it \
	--user $(id -u):$(id -g) \
	--workdir="/home/$USER" \
	--volume="/etc/group:/etc/group:ro" \
	--volume="/etc/passwd:/etc/passwd:ro" \
	--volume="/etc/shadow:/etc/shadow:ro" \
	-v ${HOME}:${HOME} \
	-v $(dirname $SSH_AUTH_SOCK):$(dirname $SSH_AUTH_SOCK) \
	-e SSH_AUTH_SOCK=$SSH_AUTH_SOCK \
	-e DISPLAY \
	-e "QT_X11_NO_MITSHM=1" \
	--volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
	--volume="$HOME/.Xauthority:/root/.Xauthority" \
	--cap-add=SYS_PTRACE \
	--privileged \
	--shm-size="1g" \
	--hostname="$IMG" \
	$IMG "$@"
)

if [ -z $CONTAINER ]; then
    echo Container start failed, ${IMG}
    exit 1
else
    echo ${IMG} $CONTAINER
    docker exec -it $CONTAINER /bin/bash -l
fi

exit 0
