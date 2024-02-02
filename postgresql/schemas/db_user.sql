-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler version: 1.0.6
-- PostgreSQL version: 12.0
-- Project Site: pgmodeler.io
-- Model Author: ---

-- Database creation must be performed outside a multi lined SQL file. 
-- These commands were put in this file only as a convenience.
-- 
-- object: ubankpgdb | type: DATABASE --
-- DROP DATABASE IF EXISTS ubankpgdb;
CREATE DATABASE ubankpgdb;
-- ddl-end --


-- object: ubank_db_1 | type: SCHEMA --
-- DROP SCHEMA IF EXISTS ubank_db_1 CASCADE;
CREATE SCHEMA ubank_db_1;
-- ddl-end --

SET search_path TO pg_catalog,public,ubank_db_1;
-- ddl-end --

-- object: ubank_db_1.client | type: TABLE --
-- DROP TABLE IF EXISTS ubank_db_1.client CASCADE;
CREATE TABLE ubank_db_1.client (
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
ALTER TABLE ubank_db_1.client OWNER TO postgres;
-- ddl-end --


