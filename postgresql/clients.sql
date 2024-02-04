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
-- CREATE DATABASE "ubank-db-1";
-- ddl-end --


-- object: bank_schema | type: SCHEMA --
DROP SCHEMA IF EXISTS bank_schema CASCADE;
CREATE SCHEMA bank_schema;
-- ddl-end --

SET search_path TO pg_catalog,public,bank_schema;
-- ddl-end --

-- object: bank_schema.clients | type: TABLE --
DROP TABLE IF EXISTS bank_schema.clients CASCADE;
CREATE TABLE bank_schema.clients (
	id serial NOT NULL,
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
	disability smallint,
	retiree bool NOT NULL,
	monthly_income integer NOT NULL,
	conscription bool NOT NULL,
	CONSTRAINT client_pk PRIMARY KEY (id)
);
-- ddl-end --


