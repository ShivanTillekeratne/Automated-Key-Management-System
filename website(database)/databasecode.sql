DROP DATABASE IF EXISTS keysystem;
CREATE DATABASE IF NOT EXISTS keysystem;
USE keysystem;
CREATE TABLE employee(
    employee_id VARCHAR(10) NOT NULL PRIMARY KEY,
    employee_name varchar(64)
);

CREATE TABLE key_event(
    finger_id VARCHAR(10) NOT NULL,
    date_ DATE NOT NULL,
    time_ TIME NOT NULL,
    key_num varchar(2) NOT NULL,
    key_taken BOOLEAN NOT NULL,
    CONSTRAINT PK PRIMARY KEY(finger_id,date_,time_,key_num,key_taken),
    CONSTRAINT fk FOREIGN KEY (`finger_id`) REFERENCES `employee`(`employee_id`) ON DELETE RESTRICT ON UPDATE RESTRICT
);
ALTER TABLE `key_event` CHANGE `date_` `date_` DATE NOT NULL DEFAULT CURRENT_TIMESTAMP;

INSERT INTO employee VALUES('e001','saman kumara');
INSERT INTO key_event VALUES('e001','','','12',0);
