<?php 

// Suppose you have the Cassandra cluster at 127.0.0.1 in default port
$builder  = new CqlBuilder();
$builder->addContactPoint("127.0.0.1");

// Now build a model of cluster and connect it to DB.
$cluster  = $builder->build();
$session  = $cluster->connect();

$keyspace = 'test_keyspace_' . rand();

// Write a query
$query    = new CqlQuery("CREATE KEYSPACE {$keyspace} WITH REPLICATION = { 'class' : 'SimpleStrategy', 'replication_factor' : 1 };");

// Send the query.
$future   = $session->query($query);

// Show the result.
echo "Keyspace $keyspace created: ";
echo (null === $future->getError() ? 'done' : 'failure') . "\n";

// Drop the new keyspace
$query->setQueryString("DROP KEYSPACE {$keyspace}");
$session->query($query);

echo "Keyspace $keyspace dropped.\n";

// Boilerplate: close the connection session and perform the cleanup.
$session->close();
$cluster->shutdown();