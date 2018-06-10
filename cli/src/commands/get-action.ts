import axios from "axios";

export default function getAction(name: string, options: any) {
    const url = options.url + "/action/" + name;
    return axios.get(url);
}
