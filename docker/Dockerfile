FROM alpine:3.21

ARG USER=vscode
ARG UID=1000
ARG GID=1000

RUN apk update --no-cache \
  && apk add --no-cache \
  bash \
  build-base \
  clang-extra-tools \
  cmake \
  gawk \
  gcompat \
  gdb \
  git \
  lcov \
  mongo-c-driver-dev \
  python3 \
  samurai \
  starship \
  sudo \
  upx \
  && adduser -D -h /home/${USER} -u ${UID} -s /bin/bash ${USER} \
  && printf "${USER}\t\tALL=(ALL)\tNOPASSWD:ALL" | tee /etc/sudoers.d/${USER} \
  && chmod 400 /etc/sudoers.d/${USER}

COPY --chmod=600 --chown=${UID}:${UID} .bashrc /home/${USER}
COPY --chmod=600 --chown=${UID}:${UID} .profile /home/${USER}

USER ${USER}

ENTRYPOINT [ "/bin/bash" ]
