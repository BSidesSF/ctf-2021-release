FROM gcr.io/bsides-sf-ctf-2021/base:latest

RUN apt-get update && \
  apt-get -yy install python3-pip python3-gunicorn gunicorn3

RUN useradd -m -g ctf flag && \
  chmod 550 /home/flag && \
  chown -R root:ctf /home/flag

COPY --chown=flag:ctf getflag /home/flag/getflag
COPY --chown=flag:ctf flag.txt /home/flag/flag.txt

COPY --chown=root:ctf src/ /home/ctf/

RUN umask 022 && pip3 install -r /home/ctf/requirements.txt

RUN chmod 4510 /home/flag/getflag && \
  chmod 400 /home/flag/flag.txt && \
  chmod 550 /home/flag && \
  chmod -R 550 /home/ctf

EXPOSE 5000

ENV AUTH_KEY="good_work_but_need_a_shell"

WORKDIR /home/ctf
USER ctf

CMD ["/usr/bin/gunicorn3", \
     "--bind=0.0.0.0:5000", \
     "--access-logfile=-", \
     "--chdir=/home/ctf", \
     "--workers=8", \
     "main:app"]
