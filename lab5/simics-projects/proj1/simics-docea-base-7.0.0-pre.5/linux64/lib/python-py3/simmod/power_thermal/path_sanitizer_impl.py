import conf  # noqa: F401 E402
import simics_6_api as simics


class PathSanitizer():
    __simics_path_separator = "."
    __docea_path_separator = "/"
    __space = " "
    __at = "@"
    __hyphen = "-"
    __underscore = "_"

    @staticmethod
    def ensure_namespace(namespace: str):
        if len(namespace) == 0:
            return
        names = namespace.split(PathSanitizer.__simics_path_separator)
        conf_path = ["conf"]
        qualified_path = []
        for name in names:
            qualified_path.append(name)
            namespace_exist = hasattr(
                eval(PathSanitizer.__simics_path_separator.join(conf_path)),
                name)
            if not namespace_exist:
                simics.SIM_add_configuration([
                    simics.pre_conf_object(
                        PathSanitizer.__simics_path_separator.join(
                            qualified_path), 'namespace')
                ], None)
            conf_path.append(name)

    @staticmethod
    def get_sanitized_path_slice(path: str):
        # Replace simics_path_separators ('.') to docea path separators ('/')
        # before splitting so that result contains all namespace elements
        # and the namespace is correctly ensured
        path = path.replace(PathSanitizer.__simics_path_separator,
                            PathSanitizer.__docea_path_separator)
        result = path.split(PathSanitizer.__docea_path_separator)
        for i in range(len(result)):
            result[i] = PathSanitizer.sanitize(result[i])
        return result

    @staticmethod
    def sanitize(name: str):
        characters_to_sanitize = {
            PathSanitizer.__space:
            PathSanitizer.__underscore,
            PathSanitizer.__at:
            PathSanitizer.__underscore,
            PathSanitizer.__hyphen:
            PathSanitizer.__underscore,
            PathSanitizer.__docea_path_separator:
            PathSanitizer.__simics_path_separator
        }
        result = name

        for docea_char, simics_replacement in characters_to_sanitize.items():
            result = result.replace(docea_char, simics_replacement)
        return result
