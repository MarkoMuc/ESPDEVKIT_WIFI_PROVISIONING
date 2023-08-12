#include "html_strings.h"

const char getHTMLString[] =  "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "    <title>WIFI CREDENTIALS</title>\n"
    "    <style>\n"
    "        .required-field::before {\n"
    "            content: \"*\";\n"
    "            color: red;\n"
    "        }\n"
    "        .required-message {\n"
    "            color: red;\n"
    "        }\n"
    "    </style>\n"
    "    <script>\n"
    "        function appendToEnd() {\n"
    "            var ssidField = document.getElementById(\"SSID\");\n"
    "            ssidField.value = ssidField.value + \"eENDd\";\n"
    "        }\n"
    "    </script>\n"
    "</head>\n"
    "<body>\n"
    "    <h1>WIFI CREDENTIALS</h1>\n"
    "    <form action=\"/creds\" method=\"post\" onsubmit=\"appendToEnd()\">\n"
    "        <label for=\"password\" class=\"required-field\">Password:</label><br>\n"
    "        <input type=\"password\" id=\"password\" name=\"password\" required><br>\n"
    "        <label for=\"SSID\" class=\"required-field\">SSID:</label><br>\n"
    "        <input type=\"text\" id=\"SSID\" name=\"SSID\" required><br><br>\n"
    "        <input type=\"submit\" value=\"Submit\">\n"
    "    </form>\n"
    "    <p class=\"required-message\">* All fields are required.</p>\n"
    "</body>\n"
    "</html>";
    
const char postHTMLString[] = "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
        <title>WIFI Credentials Received</title>\n\
    </head>\n\
    <body style=\"text-align: center; font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh;\">\n\
        <div>\n\
            <h1>WIFI credentials received</h1>\n\
            <a href=\"/\" style=\"display: block; margin-top: 20px; text-decoration: none; padding: 10px 20px; background-color: #4CAF50; color: white;\">Return</a>\n\
        </div>\n\
    </body>\n\
    </html>";
