import sys
import io


class Calcifier:
    def __init__(self):
        self.root = "root"
        self.lang = "calcifier_calc"
        self.rules = {}
        self.rules_by_priority = {}
        self.it = "it"
        self.node_type = "calcifier_calc_node_t*"
        self.vars = self.crange("a", "z")

    def priorities(self):
        return self.rules_by_priority.keys()

    @staticmethod
    def crange(_from: str, _to: str) -> str:
        return set(map(chr, range(ord(_from), ord(_to) + 1)))

    def load(self, file: io.TextIOWrapper) -> bool:
        self.__init__()

        if not file.readable():
            return False

        for src in file.read().splitlines():
            if src == None:
                break

            src = list(filter(len, map(str.strip, src.split(" "))))

            if len(src) == 0 or src[0] == '#':
                continue

            if len(src) == 1 and src[0] == "end":
                break

            if self.load_lang(src):
                continue

            if self.load_root(src):
                continue

            if self.load_vars(src):
                continue

            if self.load_it(src):
                continue

            if self.load_rule(src):
                continue

            print(f'#error "{src}"')

            return False

        return True

    def load_root(self, src: list) -> bool:
        if len(src) == 2 and src[0] == "root":
            self.root = src[1]

            return True

        return False

    def load_lang(self, src: list) -> bool:
        if len(src) == 2 and src[0] == "lang":
            self.lang = src[1]

            return True

        return False

    def load_type(self, src: list) -> bool:
        if len(src) == 2 and src[0] == "type":
            self.value_type = src[1]

            return True

        return False

    def load_vars(self, src: list) -> bool:
        if len(src) > 0 and src[0] == "vars":
            self.vars = set()

            for rng in src[1:]:
                if len(rng) == 2:
                    self.vars |= self.crange(rng[0], rng[1])
                else:
                    self.vars |= set([rng])
                    
            return True

        return False

    def load_it(self, src: list) -> bool:
        if len(src) >= 2 and src[0] == "it":
            self.it = src[1]

            return True
        
        return False

    # bug: maybe this function has any well-known name
    @staticmethod
    def zigzag(lst: list, unit: int) -> list:
        r = []
        u = []

        for i in lst:
            u += [i]

            if len(u) >= unit:
                r += [u]
                u = []

        return r

    def load_rule(self, src: list) -> bool:
        header = 7

        if not (len(src) >= header + 2
                and src[0] == "rule"
                and (len(src) - header) % 2 == 0):
            return False

        base_rule = {
            'priority': src[1],
            'order': src[2],
            'argc': int(src[3]),
            'left': src[4],
            'right': src[5],
            'next': src[6]
        }

        if base_rule['priority'] not in self.priorities():
            self.rules_by_priority[base_rule['priority']] = []

        for c, f in self.zigzag(src[header:], 2):
            rule = base_rule.copy()
            rule.update({
                'key': c,
                'function': f
            })

            self.rules[rule['key']] = rule
            self.rules_by_priority[rule['priority']] += [rule]

        return True


    def dump(self, file: io.TextIOWrapper):
        file.writelines([
            f'root {self.root}\n',
            f'vars {" ".join(self.vars)}\n',
            f'it {self.it}\n'
        ])

        for key in self.priorities():
            rules = self.rules_by_priority[key]
            r = rules[0]

            file.write(f'rule {key}' +
                f' {r["order"]} {r["argc"]}' +
                f' {r["left"]} {r["right"]} {r["next"]}')

            for rule in rules:
                file.write(f' {rule["key"]} {rule["function"]}')

            file.write("\n")

    def is_then(self, rule):
        return rule['function'] in [ "if", "then", "pyif" ]

    def is_do(self, rule):
        return rule["function"] == "do"

    def is_while(self, rule):
        return rule["function"] == "while"

    def generate(self, file: io.TextIOWrapper) -> bool:
        if not file.writable():
            return False

        if self.root not in self.priorities():
            return False

        self.generate_header(file)

        for key in self.priorities():
            self.generate_func(file, key)

        return True

    def generate_header(self, file: io.TextIOWrapper):
        keys = ", ".join(list(map(str, map(ord, self.vars))))

        file.write(f"""
#include "calcifier_rtl.h"
#include "{self.lang}.h"

typedef {self.node_type} node_t;

""")
        for key in self.priorities():
            rules = self.rules_by_priority[key]
            file.write("static " + self.func_header(rules[0]) + ";\n")

        file.write(f"""
bool {self.lang}_tryParse(char *s, ptrdiff_t len, node_t *out_value) {{
    return tryParse_{self.root}(s, len, out_value);
}}
""")

        file.write(f"""
static bool tryParseName(char *s, ptrdiff_t len, node_t *out_value) {{
    if (s == NULL || !*s || len == 0
        || !tryTrim(s, len, &s, &len)
        || len == 0)
        return false;
    if (len == 1 && strchr("{self.escape_str(self.vars)}", *s)) {{
        if (out_value) *out_value = {self.lang}_newnode_name_0(s, 1);
        return true;
    }} else if (len == {len(self.it)} && strncmp(s, "{self.escape_str(self.it)}", {len(self.it)}) == 0) {{
        if (out_value) *out_value = {self.lang}_newnode_it_0();
        return true;
    }}
    return false;
}}
static bool tryParse_n(char *s, ptrdiff_t len, node_t *out_value) {{
    if (s == NULL || !*s || len == 0
        || !tryTrim(s, len, &s, &len)
        || len == 0)
        return false;
    return {self.lang}_tryParseValue(s, len, out_value)
        || tryParseName(s, len, out_value);
}}
""")

    def func_name(self, rule) -> str:
        return f'tryParse_{rule["priority"]}'

    def func_header(self, rule) -> str:
        head = ""

        if self.is_then(rule):
            t = "Elseable"

        return (f'bool {self.func_name(rule)}' + 
            f'(char *s, ptrdiff_t len, node_t *out_value)')

    def generate_nodefuncs(self, file: io.TextIOWrapper):
        for r in self.rules:
            file.write(f'extern node_t {self.lang}_{r["key"]}_{r["argc"]}(')

            for i in range(r["argc"]):
                if i > 0:
                    file.write(", ")
                file.write("node_t")
            file.write(");\n")

    @staticmethod
    def escape_char(c):
        if ord(c) in range(33, 127) and c not in "\\\"\'":
            return c
        else:
            return "\\x{:02x}".format(ord(c))

    def escape_str(self, s):
        return "".join(list(map(self.escape_char, s)))

    def rules_to_keys(self, rules: list) -> str:
        def f(rule):
            return self.escape_char(rule["key"])

        return "".join(list(map(f, rules)))

    def generate_func(self, file: io.TextIOWrapper, priority: str) -> bool:
        rules = self.rules_by_priority[priority]
        rule = rules[0]
        skip = f'{self.lang}_skip{rule["order"]}'

        if rule["next"] == priority:
            return False

        if rule["left"] == priority and rule["right"] == priority:
            return False

        if rule["left"] == priority and rule["order"] == "l":
            return False

        if rule["right"] == priority and rule["order"] == "r":
            return False

        if rule["argc"] not in range(1, 3):
            return False

        file.write(f"""
{self.func_header(rule)} {{
    char *s2;
    ptrdiff_t len2;
    if (s == NULL || !*s || len == 0
        || !tryTrim(s, len, &s2, &len2)
        || len2 == 0)
        return false;
""")

        if rule["argc"] == 1:
            if rule["left"] != "-" and rule["right"] != "-":
                return False

            if rule["order"] == "l":
                idx = "0"
                next_idx = "s2 + 1" 
            else:
                idx = "len2 - 1"
                next_idx = "s2"

            if rule["left"] != "-":
                child = "left"
            else:
                child = "right"

            file.write(f"""
    if (strchr("{self.rules_to_keys(rules)}", s2[{idx}])
        && tryParse_{rule[child]}({next_idx}, len2 - 1, out_value))
    {{
        if (out_value) {{
            """)

            for r in rules:
                file.write(f"""if (s2[{idx}] == '{r["key"]}')
                *out_value = {self.lang}_newnode_{r["function"]}_1(*out_value, {self.lang}_nilnode());
            else """)

            file.write(f"""{{
                {self.lang}_delnode(*out_value);
                *out_value = {self.lang}_nilnode();
            }}
        }}
        return true;
    }}
    return """)

            if rule["next"] == "-":
                file.write("false")
            else:
                file.write(f"""tryParse_{rule["next"]}(s2, len2, out_value)""")
        
            file.write(""";
}""")
            return True

        if rule["argc"] != 2:
            return False

        if rule["left"] == "-" or rule["right"] == "-":
            return False

        # any binary operators
        file.write(f"""
    node_t left = {self.lang}_nilnode(), right = {self.lang}_nilnode();
""")
        if rule["order"] == "l":
            file.write(f"""
    for (ptrdiff_t opr = 0; opr < len2; opr = {skip}(s2, len2, opr)) {{""")
        else:
            file.write(f"""
    for (ptrdiff_t opr = len2; (opr = {skip}(s2, len2, opr)) > 0;) {{""")

        file.write(f"""
        left = {self.lang}_delnode(left);
        right = {self.lang}_delnode(right);
        if (!strchr("{self.rules_to_keys(rules)}", s2[opr])) continue;
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        if (tryParse_{rule["left"]}(s2, opr, &left)
            && tryParse_{rule["right"]}(right_p, right_len, &right))
        {{
            if (out_value) {{
                """)

        for r in rules:        
            file.write(f"""if (s2[opr] == '{r["key"]}')
                    *out_value = {self.lang}_newnode_{r["function"]}_2(left, right);
                else """)

        file.write(f"""{{
                    *out_value = left;
                    {self.lang}_delnode(right);
                }}
            }}
            return true;
        }}
    }}
    return """)
        if rule["next"] == "-":
            file.write('false')
        else:
            file.write(f'tryParse_{rule["next"]}(s2, len2, out_value)')

        file.write(""";
}
""")

        return True

calcifier = Calcifier()

calcifier.load(sys.stdin)
print("/*")
calcifier.dump(sys.stdout)
print("*/")
calcifier.generate(sys.stdout)
