#! /bin/sh

if [ $1 = "start" ]; then

    echo "Starting aesdsocket"
    start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket
    ;;


elif [ $1 = "stop" ]; then

    echo "Stoping aesdsocket"
    start-stop-daemon -K -n aesdsocket
    ;;
else

    echo "Usage: $0 {start | stop}"
    exit 1

fi

exit 0  
