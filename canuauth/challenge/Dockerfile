FROM gcr.io/bsides-sf-ctf-2021/base:latest

COPY ./challenge /home/ctf/challenge
COPY ./ca/chain.crt /home/ctf/chain.pem
COPY ./ca/server.key /home/ctf/server.key

ENV LISTEN_ADDR 0.0.0.0:443
ENV CERT_FILE /home/ctf/chain.pem
ENV KEY_FILE /home/ctf/server.key

EXPOSE 1300

WORKDIR "/home/ctf"

CMD ["/home/ctf/challenge"]
