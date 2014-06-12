<?php
/**
 * @file
 */

/**
 * Class CommonsMigrateGroupFieldHandler
 */
class CommonsMigrateGroupFieldHandler extends MigrateEntityReferenceFieldHandler {
  public function __construct() {
    $this->registerTypes(array('entityreference'));
  }

  public function prepare($entity, array $field_info, array $instance, array $values) {
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }
    else {
      $arguments = array();
    }
    if (empty($values[0])) {
      $values = array();
    }

    foreach ($values as $value) {
      print "Searching for " . $value . "\n";
      $query = new EntityFieldQuery();
      $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'group')
        ->propertyCondition('title', $value)
        ->propertyCondition('status', 1)
        ->range(0,1)
        ->execute();

      if (!empty($entities['node'])) {
        $existing_node = node_load(array_shift(array_keys($entities['node'])));
        print "Existing node " . $existing_node->nid . "\n";
        $nids[] = $existing_node->nid;
      }
        if (!isset($existing_node) && !empty($arguments['create_node'])) {
          $new_node = new stdClass();
          $new_node->title = $value;
          $new_node->type = 'group';
          node_object_prepare($new_node);
          $new_node->field_and_subscribe_settings = 'approval';
          $new_node->status = 1;
          $new_node->language = LANGUAGE_NONE;
          $new_node->uid = 1;
          node_save($new_node);
          print "New node " . $new_node->nid . "\n";
          $nids[] = $new_node->nid;
        }
      }

    return $nids;
    }

}