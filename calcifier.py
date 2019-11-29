import sys
import io


class Calcifier:
    def __init__(self):
        self.root = "root"
        self.rules = {}
        self.rules_by_priority = {}
        self.it = "it"
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
            if src == None or src == "end":
                break

            src = list(filter(len, map(str.strip, src.split(" "))))

            if len(src) == 0 or src[0] == '#':
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


    def load_root(self, src: list) -> bool:
        if len(src) == 2 and src[0] == "root":
            self.root = src[1]

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

        self.generate_main(file)

        return True

    def generate_header(self, file: io.TextIOWrapper):
        keys = ", ".join(list(map(str, map(ord, self.vars))))

        file.write(f"""
#include "calcifier_rtl.h"

static int var_it;
static int vars[256];

""")
        for key in self.priorities():
            rules = self.rules_by_priority[key]
            file.write("static " + self.func_header(rules[0]) + ";\n")

        file.write(f"""
bool tryParse(char *s, ptrdiff_t len, int *out_value) {{
    return tryParse_{self.root}(s, len, out_value);
}}
""")

        file.write(f"""
static bool tryParseName(char *s, ptrdiff_t len, int *out_value) {{
    char *s2;
    ptrdiff_t len2;
    if (s == NULL || !*s || len == 0
        || !tryTrim(s, len, &s2, &len2)
        || len2 == 0)
        return false;
    if (len2 == 1 && strchr("{self.escape_str(self.vars)}", *s2)) {{
        if (out_value) *out_value = *s2 % {len(self.vars)};
        return true;
    }} else if (len2 == 2 && strncmp(s2, "{self.escape_str(self.it)}", 2) == 0) {{
        if (out_value) *out_value = var_it;
        return true;
    }}
    return false;
}}
""")

    def generate_main(self, file: io.TextIOWrapper):
        file.write("""
int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;
    char *expr = argv[1];
    int result;
    if (!tryParse(expr, strlen(expr), &result)) {
        fputs("error\\n", stderr);
        return 1;
    }
    printf("%d\\n", result);
    return 0;
}
""")

    def func_name(self, rule) -> str:
        return f'tryParse_{rule["priority"]}'

    def func_header(self, rule) -> str:
        head = ""
        t = "int"

        if self.is_then(rule):
            t = "Elseable"

        return (f'bool {self.func_name(rule)}' + 
            f'(char *s, ptrdiff_t len, {t} *out_value)')

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

    def binary_functions(self):
        return {
            "add": "left + right",
            "sub": "left - right",
            "mul": "left * right",
            "div": "right ? left / right : 0",
            "mod": "right ? left % right : 0",
            "shl": "left << right",
            "shr": "left >> right",
            "equ": "left == right",
            "neq": "left != right",
            "lt": "left < right",
            "gt": "left > right",
            "lte": "left <= right",
            "gte": "left >= right",
            "max": "(left > right ? left : right)",
            "min": "(left < right ? left : right)",
            "bitand": "left & right",
            "bitor": "left | right",
            "bitxor": "left ^ right",
            "left": "left",
            "right": "right"
        }

    def generate_func(self, file: io.TextIOWrapper, priority: str) -> bool:
        rules = self.rules_by_priority[priority]
        rule = rules[0]
        find = "find"

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

        if rule["order"] == "r":
            find += "r"

        find += "Any"

        file.write(f"""
static {self.func_header(rule)} {{
    char *s2;
    ptrdiff_t len2;
    if (s == NULL || !*s || len == 0
        || !tryTrim(s, len, &s2, &len2)
        || len2 == 0)
        return false;
""")

        if rule["argc"] == 1:
            if rule["order"] == "l":
                if rule["left"] != "-" or rule["right"] != "n" or rule["next"] != "n":
                    return False
                idx = "0"
                next_idx = "s2 + 1" 
            else:
                if rule["right"] != "-" or rule["left"] != "n" or rule["next"] != "n":
                    return False
                idx = "len2 - 1"
                next_idx = "s2"

            file.write(f"""
    if (strchr("{self.rules_to_keys(rules)}", s2[{idx}])
        && {self.func_name(rule)}({next_idx}, len2 - 1, out_value))
    {{
        if (out_value) {{
            *out_value = """)

            for r in rules:
                f = r["function"]

                vals = {
                    "nop": "*out_value",
                    "neg": "-*out_value",
                    "set": "(vars[abs(*out_value % 256)] = var_it)",
                    "get": "vars[abs(*out_value % 256)]"
                }

                if f not in vals.keys():
                    file.write(f'#error "unknown function {f}"')

                    return False

                file.write(f"""s2[{idx}] == '{r["key"]}' ? {vals[f]}
                : """)

            file.write("""*out_value;
        }
        return true;
    }
    return tryParseInt(s2, len2, out_value)
        || tryParseName(s2, len2, out_value);
}
""")
            return True

        if rule["argc"] != 2:
            return False


        if rule["function"] == "else":
            if (len(rules) != 1
                    or not self.is_then(
                        self.rules_by_priority[rule["left"]][0])):
                return False

            file.write(f"""
    ptrdiff_t opr = {find}(s2, len2, "{self.rules_to_keys(rules)}");    
    if (opr < len2) {{
        Elseable left;
        if (!tryParse_{rule["left"]}(s2, opr, &left)) return false;
        if (left.success) {{
            if (out_value) *out_value = left.value;
            return true;
        }}
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        int right;
        if (!tryParse_{rule["right"]}(right_p, right_len, &right)) return false;
        if (out_value) *out_value = right;
        return true;
    }}
    return tryParse_{rule["next"]}(s2, len2, out_value);
}}
""")

            return True


        # then/while/do
        if rule["order"] == "l":
            v_before = "left"
            v_after = "right"
            pos_before = "s2, opr"
            pos_after = "right_p, right_len"
        else:
            v_before = "right"
            v_after = "left"
            pos_after = "s2, opr"
            pos_before = "right_p, right_len"

        if self.is_then(rule):
            if len(rules) != 1:
                return False

            file.write(f"""
    int left, right;
    ptrdiff_t opr = {find}(s2, len2, "{rule["key"]}");    
    if (opr < len2) {{
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        if (tryParse_{rule[v_before]}({pos_before}, &{v_before})
            && tryParse_{rule[v_after]}({pos_after}, &{v_after}))
        {{
            if (out_value) *out_value = createElseable({v_before}, {v_after});
            return true;
        }}
    }} else if (tryParse_{rule["next"]}(s2, len2, &left)) {{
        if (out_value) *out_value = createElseable(left, left);
        return true;
    }}
    if (out_value) *out_value = createElseable(false, 0);
    return false;
}}
""")
            return True

        if self.is_do(rule):
            if len(rules) > 1:
                return False

            file.write(f"""
    int {v_before};
    ptrdiff_t opr = {find}(s2, len2, "{rule["key"]}");
    if (opr < len2) {{
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        if (!tryParse_{rule[v_before]}({pos_before}, &{v_before})) return false;
        var_it = {v_before};
        return tryParse_{rule[v_after]}({pos_after}, out_value);
    }}
    return tryParse_{rule["next"]}(s2, len2, out_value);
}}
""")
            return True

        if self.is_while(rule):
            if len(rules) > 1:
                return False

            file.write(f"""
    ptrdiff_t opr = {find}(s2, len2, "{rule["key"]}");    
    if (opr < len2) {{
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        int left, right;
        while (true) {{
            if (!tryParse_{rule[v_before]}({pos_before}, &{v_before})) return false;
            if (!{v_before}) break;
            if (!tryParse_{rule[v_after]}({pos_after}, &{v_after})) return false;            
            if (out_value) *out_value = {v_after};
        }}
        return true;
    }}
    return tryParse_{rule["next"]}(s2, len2, out_value);
}}
""")
            return True

        # any binary operators
        if rule["order"] == "l":
            file.write("""
    for (ptrdiff_t opr = 0; opr < len2 - 1; ++opr) {""")
        else:
            file.write("""
    for (ptrdiff_t opr = len2; --opr > 0;) {""")

        file.write(f"""
        if (!strchr("{self.rules_to_keys(rules)}", s2[opr])) continue;
        char *right_p = s2 + opr + 1;
        ptrdiff_t right_len = len2 - opr - 1;
        int left, right;
        if (tryParse_{rule["left"]}(s2, opr, &left)
            && tryParse_{rule["right"]}(right_p, right_len, &right))
        {{
            if (out_value) {{
                *out_value = """)

        for r in rules:
            f = r["function"]
            vals = self.binary_functions()

            if f not in vals.keys():
                return False
        
            file.write(f"""s[opr] == '{r["key"]}' ? {vals[f]}
                    : """)

        file.write(f"""left;
            }}
            return true;
        }}
    }}
    return tryParse_{rule["next"]}(s2, len2, out_value);
}}
""")

        return True

calcifier = Calcifier()

calcifier.load(sys.stdin)
print("/*")
calcifier.dump(sys.stdout)
print("*/")
calcifier.generate(sys.stdout)
