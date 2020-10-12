# IdeenMemoorje
IdeenMemoorje is a small webserver that can be used as a plattform for submitting and collecting highly customisable forms. Its name is a compound word composed of the East Frisian words for ideas (Ideen) and memory (Memoorje), which can be translated as “memory of ideas”.  Users can access the system via an automatically generated link that allows them to fill in and submit a form. Administrators can then download these forms for further usage. The structure of these forms can be altered by administrators, who are also able to access and modify the system remotely.

For network functionality, the system uses [CivetWeb](https://github.com/civetweb/civetweb) and to implement the PDF-functionality for creating PDF summaries of submitted forms, it uses [libHaru](http://libharu.org/). The system also makes use of the [OpenSSL library](https://www.openssl.org/) to implement HTTPS.

# Installation
IdeenMemoorje can be installed by running the script `install.sh` in the src folder. So far the system has only been tested on Raspberry Pi OS.

# SSL Certificates
The System requires an SSL certificate to be started. The certificate must be named `certificate.pem` and saved in the src folder. The CivetWeb documentation contains [instructions](https://github.com/civetweb/civetweb/blob/master/docs/OpenSSL.md#creating-a-self-signed-certificate) on how to create a self signed SSL certificate. For public use, a SSL certificate of a Certificate Authority should be obtained.
