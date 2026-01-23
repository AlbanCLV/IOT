# Étape 1 : Construction
FROM node:18-alpine as build

WORKDIR /app

COPY package*.json ./
RUN npm install

COPY . .
# En production, Vite remplace les variables d'env par leur valeur au moment du build
# Nous passons donc les arguments de build
ARG VITE_INFLUX_URL=http://localhost:8086
ARG VITE_INFLUX_TOKEN=hRdqsdya7L2N5_Q4_AAjGodXxrjrvvMk4r_zyjKg92ZZS8S6Vmi792KXpAGxNYiVLfiQ9AADsaoEnUkdRYCXIQ==
ARG VITE_INFLUX_ORG=maison
ARG VITE_INFLUX_BUCKET=plante_data

ENV VITE_INFLUX_URL=$VITE_INFLUX_URL
ENV VITE_INFLUX_TOKEN=$VITE_INFLUX_TOKEN
ENV VITE_INFLUX_ORG=$VITE_INFLUX_ORG
ENV VITE_INFLUX_BUCKET=$VITE_INFLUX_BUCKET

RUN npm run build

# Étape 2 : Serveur Production
FROM nginx:alpine

COPY --from=build /app/dist /usr/share/nginx/html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
