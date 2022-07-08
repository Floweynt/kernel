cat $1 | sed "s/\$GIT_VERSION_INFO/$(git rev-parse --abbrev-ref HEAD)-$(git rev-parse --short HEAD)/g" > $2
