FROM gcr.io/bsides-sf-ctf-2021/base:latest

WORKDIR /home/ctf

COPY loginsvc /home/ctf/loginsvc
COPY *.html /home/ctf/
COPY jwtkey.pem /home/ctf/

RUN chown -R ctf:ctf /home/ctf

EXPOSE 5123

USER ctf

CMD ["/home/ctf/loginsvc"]
