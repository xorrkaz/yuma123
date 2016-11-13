#include <xmlstring.h>
#include "procdefs.h"
#include "agt.h"
#include "agt_cb.h"
#include "agt_timer.h"
#include "agt_util.h"
#include "agt_not.h"
#include "agt_rpc.h"
#include "dlq.h"
#include "ncx.h"
#include "ncxmod.h"
#include "ncxtypes.h"
#include "status.h"
#include "rpc.h"

#include <string.h>
#include <assert.h>

/*
    create:
* resource (key)
* alarm-type-id (key)
* alarm-type-qualifier (key)
* time-created
//resource-alarm-parameters
* is-cleared
* last-changed
* perceived-severity
* alarm-text
* list state-change
** time
** perceived-severity {type severity-with-clear!: union of perceived-severity and is-cleared above}
** alarm-text
*/
static val_value_t* find_alarm(val_value_t* alarm_list_val, char* resource_str, char* alarm_type_id_str, char* alarm_type_qualifier_str)
{
    return NULL;
}

static val_value_t* create_alarm(val_value_t* alarm_list_val, char* resource_str, char* alarm_type_id_str, char* alarm_type_qualifier_str, char* perceived_severity_str, char* alarm_text_str)
{
    obj_template_t* alarm_obj;
    obj_template_t* resource_obj;
    obj_template_t* alarm_type_id_obj;
    obj_template_t* alarm_type_qualifier_obj;
    obj_template_t* perceived_severity_obj;
    obj_template_t* alarm_text_obj;
    val_value_t* alarm_val;
    val_value_t* resource_val;
    val_value_t* alarm_type_id_val;
    val_value_t* alarm_type_qualifier_val;
    val_value_t* perceived_severity_val;
    val_value_t* alarm_text_val;
    val_value_t* number_of_alarms_val;
    status_t res;

    /*alarm*/
    alarm_obj = obj_find_child(alarm_list_val->obj,
                         "ietf-alarms",
                         "alarm");
    assert(alarm_obj);
    alarm_val=val_new_value();
    assert(alarm_val != NULL);
    val_init_from_template(alarm_val, alarm_obj);
    val_add_child(alarm_val, alarm_list_val);

    /*resource*/
    resource_obj = obj_find_child(alarm_obj,
                         "ietf-alarms",
                         "resource");
    assert(resource_obj);
    resource_val=val_new_value();
    assert(resource_val != NULL);
    val_init_from_template(resource_val, resource_obj);
    res = val_set_simval_obj(resource_val,
                         resource_obj,
                         resource_str);
    val_add_child(resource_val, alarm_val);

    /*alarm-type-id*/
    alarm_type_id_obj = obj_find_child(alarm_obj,
                         "ietf-alarms",
                         "alarm-type-id");
    assert(alarm_type_id_obj);
    alarm_type_id_val=val_new_value();
    assert(alarm_type_id_val != NULL);
    val_init_from_template(alarm_type_id_val, alarm_type_id_obj);
    res = val_set_simval_obj(alarm_type_id_val,
                         alarm_type_id_obj,
                         alarm_type_id_str);
    val_add_child(alarm_type_id_val, alarm_val);

    /*alarm-type-qualifier*/
    alarm_type_qualifier_obj = obj_find_child(alarm_obj,
                         "ietf-alarms",
                         "alarm-type-qualifier");
    assert(alarm_type_qualifier_obj);
    alarm_type_qualifier_val=val_new_value();
    assert(alarm_type_qualifier_val != NULL);
    val_init_from_template(alarm_type_qualifier_val, alarm_type_qualifier_obj);
    res = val_set_simval_obj(alarm_type_qualifier_val,
                         alarm_type_qualifier_obj,
                         alarm_type_qualifier_str);
    val_add_child(alarm_type_qualifier_val, alarm_val);


    /*perceived-severity*/
    perceived_severity_obj = obj_find_child(alarm_obj,
                         "ietf-alarms",
                         "perceived-severity");
    assert(perceived_severity_obj);
    perceived_severity_val=val_new_value();
    assert(perceived_severity_val != NULL);
    val_init_from_template(perceived_severity_val, perceived_severity_obj);
    res = val_set_simval_obj(perceived_severity_val,
                         perceived_severity_obj,
                         perceived_severity_str);
    val_add_child(perceived_severity_val, alarm_val);

    /* update ../number-of-alarms */
    number_of_alarms_val = val_find_child(alarm_list_val,"ietf-alarms","number-of-alarms");
    assert(number_of_alarms_val);
    VAL_UINT32(number_of_alarms_val)+=1;
}

static void update_alarm(val_value_t* alarm_val, char* resource_str, char* alarm_type_id_str, char* alarm_type_qualifier_str, char* severity_str, char* alarm_text_str, int enable)
{
}

int alarmctrl_event(char* resource_str, char* alarm_type_id_str, char* alarm_type_qualifier_str, char* severity_str, char* alarm_text_str, int enable)
{
    cfg_template_t* runningcfg;
    val_value_t* alarms_val;
    val_value_t* alarm_list_val;
    val_value_t* alarm_val;

    printf("%s: resource=%s - alarm-type-id=%s - alarm-type-qualifier=%s - severity=%s\n",__FUNCTION__, resource_str, alarm_type_id_str, alarm_type_qualifier_str, severity_str, alarm_text_str, enable?"on":"off");

    runningcfg = cfg_get_config_id(NCX_CFGID_RUNNING);
    assert(runningcfg!=NULL && runningcfg->root!=NULL);

    alarms_val = val_find_child(runningcfg->root,
                                "ietf-alarms",
                                "alarms");
    if(alarms_val==NULL) {
        return -1;
    }

    alarm_list_val = val_find_child(alarms_val,
                                "ietf-alarms",
                                "alarm-list");
    if(alarm_list_val==NULL) {
        return -1;
    }

    alarm_val = find_alarm(alarm_list_val, resource_str, alarm_type_id_str, alarm_type_qualifier_str);
    if(enable) {
        if(alarm_val==NULL) {
            alarm_val = create_alarm(alarm_list_val, resource_str, alarm_type_id_str, alarm_type_qualifier_str, severity_str, alarm_text_str);
        } else {
            update_alarm(alarm_val, resource_str, alarm_type_id_str, alarm_type_qualifier_str, severity_str, alarm_text_str, enable);
        }
    } else {
        if(alarm_val==NULL) {
            return -1;
        }
        update_alarm(alarm_val, resource_str, alarm_type_id_str, alarm_type_qualifier_str, severity_str, alarm_text_str, enable);
    }
    return 0;
}
