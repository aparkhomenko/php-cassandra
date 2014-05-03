<?php 

// Suppose you have the Cassandra cluster at 127.0.0.1 in default port
$builder  = new CqlBuilder();
$builder->addContactPoint("127.0.0.1");

// Now build a model of cluster and connect it to DB.
$cluster  = $builder->build();
$session  = $cluster->connect();

$keyspace = 'test_keyspace_' . rand();

// Write a query
$query    = new CqlQuery('');

$cql      = <<<CQL
CREATE KEYSPACE {$keyspace} WITH REPLICATION = { 'class' : 'SimpleStrategy', 'replication_factor' : 1 };
CQL;

$query->setQueryString($cql);
$future   = $session->query($query);
$future->wait(); // we should wait the result because use asynchronous model

$session->setKeyspace($keyspace); // USE test_keyspace_*

$cql      = <<<CQL
CREATE TABLE playlists (
  id uuid,
  song_order int,
  song_id uuid,
  title text,
  album text,
  artist text,
  PRIMARY KEY  (id, song_order ) )
CQL;

$query->setQueryString($cql);
$session->query($query)->wait();

$cql      = <<<CQL
BEGIN BATCH
INSERT INTO playlists (id, song_order, song_id, title, artist, album)
  VALUES (62c36092-82a1-3a00-93d1-46196ee77204, 1,
  a3e64f8f-bd44-4f28-b8d9-6938726e34d4, 'La Grange', 'ZZ Top', 'Tres Hombres')

INSERT INTO playlists (id, song_order, song_id, title, artist, album)
  VALUES (62c36092-82a1-3a00-93d1-46196ee77204, 2,
  8a172618-b121-4136-bb10-f665cfc469eb, 'Moving in Stereo', 'Fu Manchu', 'We Must Obey')

INSERT INTO playlists (id, song_order, song_id, title, artist, album)
  VALUES (62c36092-82a1-3a00-93d1-46196ee77204, 3,
  2b09185b-fb5a-4734-9b56-49077de9edbf, 'Outside Woman Blues', 'Back Door Slam', 'Roll Away')
APPLY BATCH;
CQL;

$query->setQueryString($cql);
$session->query($query)->wait();

// SELECT FROM playlists
$query->setQueryString("SELECT * FROM playlists");
$session->query($query);

// Send the query.
$future   = $session->query($query);

// Wait for the query to execute; retrieve the result.
$future->wait();
$result   = $future->getResult();

if (null === $future->getError()) {

	while ($result->next()) {

		echo "id: " . $result->get("id") . "\n";
		echo "song_order: " . $result->get("song_order") . "\n";
		echo "album: " . $result->get("album") . "\n";
		echo "artist: " . $result->get("artist") . "\n";
		echo "song_id: " . $result->get("song_id") . "\n";
		echo "title: " . $result->get("title") . "\n";
		echo "\n";
		
	}

}
else {
	var_dump($future->getError());
	echo "\n";
}

// Drop the new keyspace
$query->setQueryString("DROP KEYSPACE {$keyspace}");
$session->query($query)->wait();

// Boilerplate: close the connection session and perform the cleanup.
$session->close();
$cluster->shutdown();