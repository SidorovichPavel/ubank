-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler version: 1.0.6
-- PostgreSQL version: 12.0
-- Project Site: pgmodeler.io
-- Model Author: ---

-- Database creation must be performed outside a multi lined SQL file. 
-- These commands were put in this file only as a convenience.
-- 
-- object: "ubank-db-1" | type: DATABASE --
-- DROP DATABASE IF EXISTS "ubank-db-1";
CREATE DATABASE "ubank-db-1";
-- ddl-end --


-- object: bank_schema | type: SCHEMA --
-- DROP SCHEMA IF EXISTS bank_schema CASCADE;
CREATE SCHEMA bank_schema;
-- ddl-end --

SET search_path TO pg_catalog,public,bank_schema;
-- ddl-end --

-- object: bank_schema.clients | type: TABLE --
-- DROP TABLE IF EXISTS bank_schema.clients CASCADE;
CREATE TABLE bank_schema.clients (
	id uuid NOT NULL,
	first_name varchar NOT NULL,
	middle_name varchar NOT NULL,
	last_name varchar NOT NULL,
	gender smallint NOT NULL,
	passport_series varchar NOT NULL,
	passport_number varchar NOT NULL,
	issuing varchar NOT NULL,
	issuing_date date NOT NULL,
	id_number varchar NOT NULL,
	birth_address varchar NOT NULL,
	current_city varchar NOT NULL,
	current_address varchar NOT NULL,
	home_number varchar,
	mobile_number varchar,
	email varchar,
	post varchar,
	place_of_work varchar,
	city_of_residence varchar NOT NULL,
	residence_address varchar NOT NULL,
	family_status smallint NOT NULL,
	citizenship varchar NOT NULL,
	disability smallint NOT NULL,
	retiree bool NOT NULL,
	monthly_income integer NOT NULL,
	conscription bool NOT NULL,
	CONSTRAINT client_pk PRIMARY KEY (id),
	CONSTRAINT unique_id_number UNIQUE (id_number),
	CONSTRAINT unique_passport UNIQUE (passport_series,passport_number)
);
-- ddl-end --

-- object: bank_schema.deposits | type: TABLE --
DROP TABLE IF EXISTS bank_schema.deposits CASCADE;
CREATE TABLE bank_schema.deposits (
	id uuid NOT NULL,
	category varchar NOT NULL,
	agreement_number varchar NOT NULL,
	currency smallint NOT NULL,
	program_begin date NOT NULL,
	program_end date NOT NULL,
	agreement_begin date NOT NULL,
	agreement_end date NOT NULL,
	amount bigint NOT NULL,
	interest smallint NOT NULL,
	id_clients uuid,
	CONSTRAINT deposit_pk PRIMARY KEY (id)
);
-- ddl-end --

-- object: clients_fk | type: CONSTRAINT --
ALTER TABLE bank_schema.deposits DROP CONSTRAINT IF EXISTS clients_fk CASCADE;
ALTER TABLE bank_schema.deposits ADD CONSTRAINT clients_fk FOREIGN KEY (id_clients)
REFERENCES bank_schema.clients (id) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: bank_schema.account | type: TABLE --
DROP TABLE IF EXISTS bank_schema.account CASCADE;
CREATE TABLE bank_schema.account (
	id uuid NOT NULL,
	id_number bigint NOT NULL,
	code smallint NOT NULL,
	activity varchar NOT NULL,
	debit bigint NOT NULL,
	credit bigint NOT NULL,
	balance bigint NOT NULL,
	note varchar,
	id_deposits uuid,
	CONSTRAINT account_pk PRIMARY KEY (id)
);
-- ddl-end --

-- object: deposits_fk | type: CONSTRAINT --
ALTER TABLE bank_schema.account DROP CONSTRAINT IF EXISTS deposits_fk CASCADE;
ALTER TABLE bank_schema.account ADD CONSTRAINT deposits_fk FOREIGN KEY (id_deposits)
REFERENCES bank_schema.deposits (id) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --


