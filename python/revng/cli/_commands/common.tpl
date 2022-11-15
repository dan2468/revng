{% macro graph(elements) %}
```dot
digraph {
  node [shape=box]
  kinddir = BT;

{% for element in elements %}
   "{{ element.name }}";
{% endfor %}

{% for element in elements %}
{% if element.parent %}
   "{{ element.parent | name }}" -> "{{element.name}}";
{% endif %}
{% endfor %}
}
```
{% endmacro %}

{% macro maybe(name, value) %}
{% if value %}
**{{name}}**: {{ value | link }}
{% endif %}
{% endmacro %}

{% macro shortlist(name, elements) %}
{% if elements %}
**{{name}}**: {% for element in elements %}{% if loop.index > 1 %}, {% endif %}{{ element | link }}{% endfor %}
{% endif %}
{% endmacro %}

{% macro longlist(name, elements) %}
{% if elements %}
**{{name}}**:
{% for element in elements %}
- {{ element | link }}
{% endfor %}
{% endif %}
{% endmacro %}

