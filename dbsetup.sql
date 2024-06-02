-- MariaDB dump 10.19  Distrib 10.11.6-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: mini_projects
-- ------------------------------------------------------
-- Server version	10.11.6-MariaDB-0+deb12u1
--
-- Table structure for table `items`
--

DROP TABLE IF EXISTS `items`;
CREATE TABLE `items` (
  `srno` int(11) NOT NULL AUTO_INCREMENT,
  `item` varchar(1000) NOT NULL,
  `price` int(11) NOT NULL,
  PRIMARY KEY (`srno`),
  UNIQUE KEY `item` (`item`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Table structure for table `contacts`
--

DROP TABLE IF EXISTS `contacts`;
CREATE TABLE `contacts` (
  `name` varchar(30) NOT NULL,
  `phone` varchar(20) NOT NULL,
  UNIQUE KEY `phone` (`phone`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Table structure for table `quiz`
--

DROP TABLE IF EXISTS `quiz`;
CREATE TABLE `quiz` (
  `srno` int(11) NOT NULL AUTO_INCREMENT,
  `question` varchar(1000) NOT NULL,
  `answer` varchar(5000) NOT NULL,
  `year` int(11) DEFAULT NULL,
  PRIMARY KEY (`srno`),
  UNIQUE KEY `question` (`question`) USING HASH
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- Dump completed on 1920-01-01 01:01:01

