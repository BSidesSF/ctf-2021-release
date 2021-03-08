FROM ruby:2.7.0

RUN apt-get update
RUN apt-get install -y net-tools ruby-bundler gdb nasm

# Copy up the client binary (for validation)
COPY client/secure-asset-manager /secure-asset-manager
RUN chmod +r /secure-asset-manager

# Copy up the server
RUN mkdir /server
COPY server/ /server
RUN chmod +r /server/*
RUN chmod +x /server/not-solution
WORKDIR /server
RUN bundle install

# Add a user
RUN useradd user
RUN chown -R user /server

# Set up ENV path
ENV REAL_BINARY /secure-asset-manager
ENV CHECKER /server/not-solution
ENV EASTER_EGG /server/easter-egg.jpg

# Start server
CMD ["ruby", "./server.rb"]
