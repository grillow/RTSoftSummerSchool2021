FROM node:latest

ENV NODE_ENV=production
WORKDIR /usr/src/4-consumer

COPY package*.json ./
RUN npm ci --only=production

COPY . .

CMD npm start
