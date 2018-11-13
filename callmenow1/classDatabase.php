<?php
/**
 * Class handeling basic database interactions
 * 
 * Swaps over to a secondary server when primairy fails
 *
 */
class Database
{
	private $PrimaryServerSettings;
	private $SecondaryServerSettings;
	private $UseDB;

	/**
	 * 	Set the database to use
	 */
	public function SetDatabase($DataBase)
	{
		$this->UseDB = $DataBase;
		return;
	}
	
	/**
	 * Set the values of the primary slqserver
	 * ARG:
	 * 	ServerName		-	Name of the server to connect to (can be an IP address)
	 * 						This name must correspond with the name in the freetds.conf
	 * 	UserName		-	Name of the user that connects to the sqlserver.
	 * 						This user must have read/write access on the database selected in SetDatabase
	 * 	UserPassword	-	Password for user
	 */
	public function SetPrimaryServerSettings($ServerName, $UserName, $UserPassword)
	{
		$this->PrimaryServerSettings = array("Server"=>$ServerName, "UserName"=>$UserName, "Password"=>$UserPassword);
		return;
	}
	
	/**
	 * Set the values of the secondary slqserver
	 * ARG:
	 * 	ServerName		-	Name of the server to connect to (can be an IP address)
	 * 						This name must correspond with the name in the freetds.conf
	 * 	UserName		-	Name of the user that connects to the sqlserver.
	 * 						This user must have read/write access on the database selected in SetDatabase
	 * 	UserPassword	-	Password for user
	 */
	public function SetSecondaryServerSettings($ServerName, $UserName, $UserPassword)
	{
		$this->SecondaryServerSettings = array("Server"=>$ServerName, "UserName"=>$UserName, "Password"=>$UserPassword);
		return;
	}
	
	/**
	 * Create a connection to a database
	 * If the primary fails try secondary
	 * 
	 * Returns: Mixed
	 * 	False when no connection to either servers/dbase can be made
	 * 	connection identiefier when connection was made.
	 */
	private function MakeConnectionToServer()
	{
		$Server = $this->PrimaryServerSettings;
		$conn = mssql_connect($Server["Server"], $Server["UserName"], $Server["Password"]);
		if(!$conn)
		{
			print "Could not connect to the primary database";
			$Server = $this->SecondaryServerSettings;
			$conn = mssql_connect($Server["Server"], $Server["UserName"], $Server["Password"]);
		}
		return $conn;
	}
	
	/**
	 * Execute a query on a database
	 * Stores the result in the ResultSet variable
	 * 
	 * Returns
	 * 	True when there is result
	 * 	False when there was a problem connection to server or database
	 * 		
	 */
	public function GoSQL($SQLQuery, &$ResultSet)
	{
		$bRetVal = false;
		$ServerConnection = $this->MakeConnectionToServer();
		
		if($ServerConnection)
		{
			if(mssql_select_db($this->UseDB, $ServerConnection))
			{
				$bRetVal = true;
				$ResultSet = mssql_query($SQLQuery,$ServerConnection);
			}
		}
		else
			print "Could not connect to a database";
		return $bRetVal;
	}
}
?>