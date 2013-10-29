# Compile ProxyServer.java
compile-linux: lib/httpcore-4.3.jar lib/httpclient-4.3.1.jar lib/commons-codec-1.6.jar lib/httpmime-4.3.1.jar lib/commons-logging-1.1.3.jar lib/httpclient-cache-4.3.1.jar lib/fluent-hc-4.3.1.jar ProxyServer.java
	javac -cp ".:lib/httpcore-4.3.jar:lib/httpclient-4.3.1.jar:lib/commons-codec-1.6.jar:lib/httpmime-4.3.1.jar:lib/commons-logging-1.1.3.jar:lib/httpclient-cache-4.3.1.jar:lib/fluent-hc-4.3.1.jar" ProxyServer.java

compile-windows: lib/httpcore-4.3.jar lib/httpclient-4.3.1.jar lib/commons-codec-1.6.jar lib/httpmime-4.3.1.jar lib/commons-logging-1.1.3.jar lib/httpclient-cache-4.3.1.jar lib/fluent-hc-4.3.1.jar ProxyServer.java
	javac -cp ".;lib/httpcore-4.3.jar;lib/httpclient-4.3.1.jar;lib/commons-codec-1.6.jar;lib/httpmime-4.3.1.jar;lib/commons-logging-1.1.3.jar;lib/httpclient-cache-4.3.1.jar;lib/fluent-hc-4.3.1.jar" ProxyServer.java

# Run ProxyServer
run-linux: lib/httpcore-4.3.jar lib/httpclient-4.3.1.jar lib/commons-codec-1.6.jar lib/httpmime-4.3.1.jar lib/commons-logging-1.1.3.jar lib/httpclient-cache-4.3.1.jar lib/fluent-hc-4.3.1.jar ProxyServer.class
	java -cp ".:lib/httpcore-4.3.jar:lib/httpclient-4.3.1.jar:lib/commons-codec-1.6.jar:lib/httpmime-4.3.1.jar:lib/commons-logging-1.1.3.jar:lib/httpclient-cache-4.3.1.jar:lib/fluent-hc-4.3.1.jar" ProxyServer

run-windows: lib/httpcore-4.3.jar lib/httpclient-4.3.1.jar lib/commons-codec-1.6.jar lib/httpmime-4.3.1.jar lib/commons-logging-1.1.3.jar lib/httpclient-cache-4.3.1.jar lib/fluent-hc-4.3.1.jar ProxyServer.class
	java -cp ".;lib/httpcore-4.3.jar;lib/httpclient-4.3.1.jar;lib/commons-codec-1.6.jar;lib/httpmime-4.3.1.jar;lib/commons-logging-1.1.3.jar;lib/httpclient-cache-4.3.1.jar;lib/fluent-hc-4.3.1.jar" ProxyServer

# Clear
clear: ProxyServer.class
	rm ProxyServer.class
