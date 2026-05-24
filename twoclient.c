#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main (){
    
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons (8080),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 2);

    
    int p1_fd = accept(listen_fd, NULL, NULL);
    printf("Jogador 1 conectado! fd=%d\n", p1_fd);

    printf("Aguardando jogador 2\n");
    int p2_fd = accept(listen_fd, NULL, NULL);
    printf("Jogador 2 conectado! fd=%d", p2_fd);

    
    fd_set master_set;
    FD_ZERO(&master_set);
    FD_SET(p1_fd, &master_set);
    FD_SET(p2_fd, &master_set);

    int max_fd = p2_fd;

    
    char buf[256];
    fd_set tmp_set;

    while(1){
        tmp_set = master_set;

        int ready = select(max_fd + 1, &tmp_set, NULL, NULL, NULL);

        if (ready == -1){
            perror("Select() falhou");
            break;
        }
        for (int fd = 0; fd <= max_fd; fd++){
            if (!FD_ISSET(fd, &tmp_set)) continue;

            int bytes = recv(fd, buf, sizeof(buf) - 1, 0);

            if (bytes == 0){
                printf("fd=%d desconectou\n", fd);
                FD_CLR(fd, &master_set);
                close(fd);
                continue;
            }
            buf[bytes] = '\0';
            printf("Recebido de fd=%d: %s\n", fd, buf);

            //lógica do jogo
        }
    }
    close(listen_fd);
    return 0;
}
