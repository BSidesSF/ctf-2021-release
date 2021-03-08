FROM debian:10-slim

RUN apt-get update && apt-get install -y xinetd ruby bundler

RUN mkdir /src
WORKDIR /src

# Source files
ADD first-names.txt flag.rb Gemfile Gemfile.lock hangman.rb last-names.txt words.txt /src/
RUN chmod +x /src/hangman.rb
RUN bundle install

# Service file
ADD service.xinetd /etc/xinetd.d/service

USER root
EXPOSE 2121

CMD service xinetd restart && sleep infinity
