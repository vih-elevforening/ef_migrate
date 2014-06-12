<?php
class EFUsers extends Migration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import users from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'uid' => array(
          'type' => 'int',
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationUser::getKeySchema()
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'ef_migrate') . '/import/users.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationUser();

    // Open Atrium
    // $this->addFieldMapping('field_user_display_name', 'full_name');

    // Commons
    $this->addFieldMapping('field_name_first', 'given_name');
    $this->addFieldMapping('field_name_last', 'surname');

    $this->addFieldMapping('name', 'username')
      ->defaultValue('')
      ->description(t('Username'));

    $this->addFieldMapping('mail', 'email')
      ->defaultValue('no-email' . uniqid() .'@invalid-email.dk')
      ->description(t('Email address'));

    $this->addFieldMapping('pass', 'password')
      ->defaultValue('vih-1942')
      ->description(t("User's password (plain text)"));

    $this->addFieldMapping('status')
      ->defaultValue(true)
      ->description(t("Is user active?"));

    $this->addFieldMapping('og_user_node', 'year')
      ->sourceMigration('EFGroups');

    $this->addFieldMapping('timezone')
      ->defaultValue('Europe/Copenhagen');

    // Custom fields added

    // Adress fields to be added
    $arguments = array(
      // 'name_line' => array('source_field' => 'full_name'),
      'thoroughfare' => array('source_field' => 'address'),
      'locality' => array('source_field' => 'city'),
      'postal_code' => array('source_field' => 'zipcode'),
      'country' => array('source_field' => 'country'),
    );
    $this->addFieldMapping('field_address', 'country')
      ->arguments($arguments)
      ->description('Address field');

    $this->addFieldMapping('field_phone', 'phone')
      ->description('Phone field');
  }

  function prepareRow($row) {
    if (empty($row->given_name) && !empty($row->full_name)) {
      $name = $this->splitName($row->full_name);
      $row->given_name = $name['given_name'];
    }
    if (empty($row->surname) && !empty($row->full_name)) {
      $name = $this->splitName($row->full_name);
      $row->surname = $name['surname'];
    }
    if (!is_int($row->id)) {
      // We should choose something specific here - or convert the id with a specific algorithm
    }

    $row->country = 'DK';

    $row->year = explode(',', $row->year);
  }

  function csvcolumns() {
    $columns[0] = array('uid', 'UID');
    $columns[1] = array('username', 'Username');
    $columns[2] = array('given_name', 'Adresse - Fornavn');
    $columns[3] = array('surname', 'Adresse - Efternavn');
    $columns[4] = array('full_name', 'Adresse - Fulde navn');
    $columns[5] = array('country', 'Adresse - Land');
    $columns[6] = array('city', 'Adresse - Locality (i.e. City)');
    $columns[7] = array('zipcode', 'Adresse - Postnummer');
    $columns[8] = array('full_zip', 'Adresse - Premise (i.e. Apartment / Suite number)');
    $columns[9] = array('address', 'Adresse - Thoroughfare (i.e. Street Address');
    $columns[10] = array('picture', 'Billede');
    $columns[11] = array('email', 'E-mail');
    $columns[12] = array('membernumber', 'Medlemsnummer');
    $columns[13] = array('year', 'Årgang');
    $columns[14] = array('phone', 'Telefon');
    return $columns;
  }

  protected function splitName($name) {
    $count = strrpos($name, ' ');
    $given_name = trim(substr($name, 0, $count));
    $surname = trim(substr($name, $count));
    return array(
      'given_name' => $given_name,
      'surname' => $surname);
  }
}

class EFGroups extends Migration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Import groups from CSV file.');

    // Create a map object for tracking the relationships between source rows
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'ef_migrate') . '/import/groups.csv', $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = new MigrateDestinationNode('group');

    $this->addFieldMapping('title', 'name');
    $this->addFieldMapping('field_and_subscribe_settings')
      ->defaultValue('approval');

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
    $this->addFieldMapping('status', 'published')->defaultValue(1);
  }

  function csvcolumns() {
    $columns[0] = array('id', 'Id');
    $columns[1] = array('name', 'Navn');
    return $columns;
  }

  function prepareRow($row) {
    $row->uid = 1;
    $row->language = LANGUAGE_NONE;
    $row->published = 1;
  }
}
