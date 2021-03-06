SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: account; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE account (
    id integer NOT NULL,
    login bytea NOT NULL,
    password bytea NOT NULL,
    date bigint NOT NULL,
    email text
);


--
-- Name: account_register; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE account_register (
    login bytea NOT NULL,
    password bytea NOT NULL,
    email text NOT NULL,
    key text NOT NULL,
    date bigint NOT NULL
);

--
-- Name: account_login_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY account
    ADD CONSTRAINT account_login_key PRIMARY KEY (login);

--
-- Name: account_register_login_key; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY account_register
    ADD CONSTRAINT account_register_login_key UNIQUE (login);


REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--
