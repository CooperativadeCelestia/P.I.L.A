#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <sstream>

using namespace boost::asio;
using ip::tcp;

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string getContentType(const std::string& filePath) {
    if (filePath.find(".html") != std::string::npos)
        return "text/html";
    else if (filePath.find(".css") != std::string::npos)
        return "text/css";
    else if (filePath.find(".js") != std::string::npos)
        return "application/javascript";
    else
        return "text/plain";
}

std::string generateResponse(const std::string& content, const std::string& contentType) {
    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;
    return response.str();
}

void handleRequest(tcp::socket& socket) {
    boost::asio::streambuf requestBuffer;
    boost::system::error_code error;

    // Read the request from the client
    boost::asio::read_until(socket, requestBuffer, "\r\n\r\n", error);
    if (error) {
        std::cout << "Error reading request: " << error.message() << std::endl;
        return;
    }

    // Extract the requested file path from the request
    std::istream requestStream(&requestBuffer);
    std::string requestLine;
    std::getline(requestStream, requestLine);
    std::string filePath = requestLine.substr(requestLine.find("GET ") + 4, requestLine.find(" HTTP/") - (requestLine.find("GET ") + 4));

    // Remove any query parameters from the file path
    if (filePath.find("?") != std::string::npos)
        filePath = filePath.substr(0, filePath.find("?"));

    // Prepend a "." to the file path to make it relative to the current directory
    filePath = "." + filePath;

    // Read the file content
    std::string fileContent = readFile(filePath);

    // Determine the content type based on the file extension
    std::string contentType = getContentType(filePath);

    // Generate the HTTP response
    std::string response = generateResponse(fileContent, contentType);

    // Send the response to the client
    boost::asio::write(socket, boost::asio::buffer(response), error);
    if (error) {
        std::cout << "Error sending response: " << error.message() << std::endl;
        return;
    }
}

int main() {
    boost::asio::io_context ioContext;

    // Create and bind the server socket
    tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), 8080));

    while (true) {
        // Wait for and accept client connections
        tcp::socket socket(ioContext);
        acceptor.accept(socket);

        // Handle the client request
        handleRequest(socket);
    }

    return 0;
}
