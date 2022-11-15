<!--- - This template file is distributed under the MIT License. See LICENSE.md for details. - --->
<!--- - The notice below applies to the generated files. - --->

<!-- - macro process_struct(struct) - -->
### <a id="{{ struct.name }}"></a>`{{ struct.name }}`

<!-- if struct.doc - -->**Description**: {{struct.doc}}<!-- - endif -->

<!-- if struct.inherits -->
**Inherits from**: {{struct.inherits | type_name}}
<!-- endif -->

<!-- if struct.key_fields -->
**Key**: <!-- for field in struct.key_fields - --><!-- if loop.index > 1 -->, <!-- endif -->`{{field.name}}`<!-- endfor -->
<!-- endif -->

<!-- if struct.abstract -->
**Inheritors**: <!-- for inheritor in struct.inheritors - --><!-- if loop.index > 1 -->, <!-- endif -->{{inheritor | type_name}}<!-- endfor -->
<!-- endif -->

**Fields**:

<!-- for field in struct.fields - -->
- `{{field.name}}` ({{field.resolved_type | type_name}})<!-- if field.doc -->: {{field.doc | indent(1) }}<!-- endif -->
<!-- endfor -->
<!-- endmacro - -->

<!-- - macro process_enum(enum) - -->
### <a id="{{ enum.name }}"></a>`{{ enum.name }}`

<!-- if enum.doc - -->**Description**: {{enum.doc}}<!-- - endif -->

**Members**:

<!-- for member in enum.members - -->
- `{{member.name}}`<!-- if member.doc -->: {{member.doc | indent(1) }}<!-- endif -->
<!-- endfor - -->
<!-- - endmacro - -->

## Data structures

<!-- for struct in structs - -->
<!-- - if struct.name == root_type - -->
{{ process_struct(struct) }}
<!-- - endif - -->
<!-- endfor - -->

<!-- - for struct in structs - -->
<!-- - if struct.name != root_type - -->
{{ process_struct(struct) }}
<!-- - endif - -->
<!-- endfor - -->

## Enumerations

<!-- for enum in enums - -->
{{ process_enum(enum) }}
<!-- - endfor - -->
