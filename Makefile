DC=docker-compose
CT=vm

all: run

run: start exec

start: 
	$(DC) up -d --build

clean: 
	$(DC) down

re: clean start

exec: 
	$(DC) exec $(CT) bash