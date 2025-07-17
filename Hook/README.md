# Jafg Hook Tool

This tool is a hook post-push system for git. It runs after a push to the git master repository was done.
It is meant to update binaries or other meta-data automatically.

The docker image will take roughly 3.2 GB of disk space, and the docker containers will take roughly 200 MB of disk space each.
The docker caching will take roughly 3.4 GB of additional disk space. Totaling with one container to roughly 6.8 GB of disk space.


# Getting up and running

1. Create a `.env` file in this directory `Hook/` with the following content:
   ```env
   SECRET_X_HUB_SIGNATURE=<your_secret>
   VOLUME_CACHE=/host
   VCS_LOCAL_REMOTE_CLONE=<path_to_your_local_clone>
   VCS_REMOTE_URL=<your_remote_url>
   ```
2. Create a directory on your host machine for caching between containers, e.g.:
   ```bash
   mkdir -p /path/to/cache
   ```
   This is needed because the Docker mount will not create the directory automatically. Please visit the [official docker docs](https://docs.docker.com/engine/storage/bind-mounts) for more information. Note that you technically do not need to use a `VOLUME_CACHE` bind to the host machine (you may also bind to the local docker image), but it is recommended to keep the cache persistent across container restarts (as starting the service (and therefore recompiling the motor) might take an extraordinary amount of time depending on the server specs).
3. Run the following command to build the Docker image:
   ```bash
   docker build -t jafg-hook .
   ```
4. And run it with:
   ```bash
   docker run --name jafg-hook
      -d
      -p <HOST>:8000
      --restart=unless-stopped
      --env-file .env
      --mount type=bind,src=<VOLUME_CACHE>,dst=/host
      jafg-hook
   ```
5. Make it available via a reverse proxy, e.g. nginx:
   ```bash
   apt install nginx
   systemctl enable nginx
   systemctl start nginx
   ```
   Then create a new configuration file in `/etc/nginx/sites-available/` (e.g. `jafg-hook.conf`) with the following content:
   ```nginx
   server
   {
      client_max_body_size 64M;
      listen 80;
      server_name <your_domain_or_ip>;
   
      # Trailing slash is important.
      location /hooks/jafg/
      {
         proxy_http_version       1.1;

         proxy_pass               http://127.0.0.1:8000/; # Trailing slash is important.
         proxy_read_timeout       60;
         proxy_connect_timeout    60;
         proxy_redirect           off;

         proxy_set_header    Host                   $host;
         proxy_set_header    X-Real-IP              $remote_addr;
         proxy_set_header    X-Forwarded-For        $proxy_add_x_forwarded_for;
         proxy_set_header    X-Forwarded-Proto      $scheme;
      }
   }
   ```
   Soft link it to the `sites-enabled` directory to enable it:
   ```bash
   ln -s /etc/nginx/sites-available/<conf> /etc/nginx/sites-enabled/
   ```
   And restart the nginx service daemon:
   ```bash
    systemctl restart nginx
   ```
6. Encrypt it with an SSL certificate, e.g., using [Certbot](https://certbot.eff.org/):
   ```bash
   apt install certbot python3-certbot-nginx
   certbot --nginx -d <your_domain_or_ip>
   ```
   And restart nginx to apply the changes made by Certbot:
   ```bash
   systemctl restart nginx
   ```


# Reload the server
If you change the code, you can reload the server by running:
```bash
docker stop jafg-hook && docker rm jafg-hook && <...>
```

When using nginx and change configs, you must also reload the nginx configuration:
```bash
systemctl reload nginx
```


# Follow the logs
To follow the logs of the running container, you can use:
```bash
docker logs -f jafg-hook
```
