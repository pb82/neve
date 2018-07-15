import axios from "axios";

export default function resolveToken(token: string, options: any) {
    const url = options.url + "/resolve/" + token;
    return axios.get(url);
}
