#!/bin/bash
# Copyright mzoesch. All rights reserved.

#
# An automated script to setup the content server for Jafg serving large files.
#

set -eo pipefail

if [ -z "$site" ]; then
    site="jafg-content"
fi

if [ -z "$server_name" ]; then
    echo "Not defined: <server_name>"
    exit 1
fi

if [ -z "$port" ]; then
    port="80"
fi

# Should include trailing slash.
if [ -z "$jafg_content_root" ]; then
    echo "Not defined: <jafg_content_root>"
    exit 1
fi

# Should include trailing slash.
if [ ! -d "$jafg_content_location" ]; then
    echo "Not found: <jafg_content_location>"
    exit 1
fi

if [ -z "$certbot_email" ]; then
    echo "Not defined: <certbot_email>"
    exit 1
fi

if [ "$EUID" -ne 0 ]; then
    echo "Not root."
    exit 1
fi

cd /home

apt update -y
apt install nginx -y

systemctl enable nginx
systemctl start nginx

echo "
server
{
    listen $port;
    server_name $server_name;

    location $jafg_content_location
    {
        alias $jafg_content_root;
        autoindex off;
    }
}
" > /etc/nginx/sites-available/$site
ln -s /etc/nginx/sites-available/$site /etc/nginx/sites-enabled/$site
nginx -t && systemctl reload nginx

apt install certbot python3-certbot-nginx -y
nginx -t && certbot --nginx -n --agree-tos --email $certbot_email --no-eff-email -d $server_name
