FROM gcr.io/bsides-sf-ctf-2021/base:latest

WORKDIR /home/ctf

COPY img/ /home/ctf/img/
COPY static/ /home/ctf/static/
COPY challenge /home/ctf/challenge
COPY jwtkey.pem /home/ctf/jwtkey.pem
COPY *.html /home/ctf/

RUN chown -R ctf:ctf /home/ctf

USER ctf

EXPOSE 5124

CMD ["/home/ctf/challenge"]
