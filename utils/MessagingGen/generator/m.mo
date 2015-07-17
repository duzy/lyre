template cxx_message_field
--------------------------
.   if .type eq 'string'
    std::$(.type) $(.name);
.elsif .type eq 'strings'
    std::list<std::$(.type)> $(.name);
.elsif .type eq 'uint8'
    uint8_t $(.name);
.elsif .type eq 'uint16'
    uint16_t $(.name);
.elsif .type eq 'uint32'
    uint32_t $(.name);
.elsif .type eq 'int8'
    int8_t $(.name);
.elsif .type eq 'int16'
    int16_t $(.name);
.elsif .type eq 'int32'
    int32_t $(.name);
.else
    ??? $(.name); // $(.type)
.end
-----------------------end

template cxx_protocol
---------------------
/** -*- c++ -*-
 *  The $(.name) protocol. This file is 100% generated, don't change it unless
 *  you really understand what you're doing.
 */
namespace $(.name)
{
    namespace messages
    {
.if +message < 256 ;
        typedef uint8_t tag_value_base;
.else;
        typedef uint16_t tag_value_base;
.end;

        enum class tag : tag_value_base
        {
.for message ;
            $(.name) = $(.id),
.end;
        };

.for message ;
        struct $(.name)
        {
            constexpr static tag TAG = tag::$(.name);
.  for ->field ;
            $(strip(str cxx_message_field))
.  end ;
        };

.end;

        template < class P, class accessor >
        struct codec
        {
            typedef messages::tag_value_base        tag_value_base;
            typedef messages::tag                   tag;

.for message ;
            static std::size_t size (P *p, const $(.name) & m)
            {
.  var $fields = +->field
.  if $fields == 0 ;
                return 0;
.  elsif $fields == 1 ;
                return accessor::field_size (p, m.$(->field[0].name));
.  else;
                return accessor::field_size (p, m.$(->field[0].name))
.    for slice(->field, 1, $fields-2) ;
                   +   accessor::field_size (p, m.$(.name))
.    end;
                   +   accessor::field_size (p, m.$(->field[$fields-1].name));
.  end;
            }
            static void encode (P *p, const $(.name) & m)
            {
.  for ->field ;
                accessor::put (p, m.$(.name));
.  end;
            }
            static void decode (P *p, const $(.name) & m)
            {
.  for ->field ;
                accessor::get (p, m.$(.name));
.  end;
            }

.end;
        };

    }
}
------------------end

template cxx_machine_state_events
---------------------------------
.for state
.for ->event
    struct $(.name) : sc::event<$(.name)> {};
.end
.for ->state
    $(str cxx_machine_state_events)
.end
.end
------------------------------end

template cxx_machine
---------------------
/** -*- c++ -*-
 *  The $(.name). This file is 100% generated, don't change it unless
 *  you really understand what you're doing.
 */
namespace $(.name) 
{
$(strip(str cxx_machine_state_events))

.for state
    $(.name)
.end
}
------------------end

def write_file($filename, $content) {
    var $file = open($filename, 'w');
    $file.print($content);
    $file.close();
}

def gen_component($_) {
    var $name = .name;
    if +->message {
        write_file("gen/$(.name)_protocol.inc", str cxx_protocol)
    }
    if +->state {
        write_file("gen/$(.name).inc", str cxx_machine)
    }
    for ->component {
        .set('parent_name', $name);
        .set('name', "$($name)_$(.name)");
        gen_component($_);
    }
}

gen_component($_)

␤
