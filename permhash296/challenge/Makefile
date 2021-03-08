GO=go

main: challenge test

test: test.go
	GOPATH=`pwd` $(GO) build test.go

challenge: challenge.go
	GOPATH=`pwd` $(GO) build challenge.go

clean:
	rm -f test challenge
	rm -f *.o
	rm -f *~
	rm -f \#*
