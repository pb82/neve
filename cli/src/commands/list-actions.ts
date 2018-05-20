import axios from "axios";

export default function listActions(options: any) {
    const url = options.url + "/action";
    return axios.get(url);
}